#include <iostream>
#include <vector>
#include <cstdlib>
#include <mutex>
#include <tgbot/tgbot.h>


using namespace std;
using namespace TgBot;

template<typename MessageType, typename = enable_if_t<is_copy_constructible<MessageType>::value>>
class LinkBin{
    mutex queue_mutex;
    vector<MessageType> to_save;
public:
    void save(MessageType m){
        lock_guard<mutex> lock(queue_mutex);
        to_save.push_back(move(m));
    }

    vector<MessageType> recap(){
        lock_guard<mutex> lock(queue_mutex);
        vector<MessageType> res;
        swap(to_save, res);
        return res;
    }


};


int main(){

    //this catch some symbols with a point in the middle, basically
    //R"reg([^\s]+\.[\w]{2,})reg";

    LinkBin<string> linkstore;

    auto token=getenv("TELEGRAM_TOKEN");
    if(token == nullptr){
        cerr << "error while reading configuration: no TELEGRAM_TOKEN ENV\n";
        throw;
    }



    Bot bot(token);

//const messagelistener cast added just to silence clion warning
    bot.getEvents().onCommand("recap", (const EventBroadcaster::MessageListener &) [&](auto message) {
            bot.getApi().sendMessage(message->chat->id, "recapping!");
            auto links=linkstore.recap();
            if(links.empty()){
                bot.getApi().sendMessage(message->chat->id, "no links for the moment :(");
                return;
            }
            ostringstream response_s;
            for(auto l : links){
                response_s << l << '\n';
            }

            bot.getApi().sendMessage(message->chat->id, response_s.str());
        });

    bot.getEvents().onNonCommandMessage((const EventBroadcaster::MessageListener &) [&](auto message) {
            cout << "User wrote: " << message->text << '\n';
            auto& text=message->text;
            for(auto en: message->entities){
                if(en->type == "url"s){
                    auto url =message->text.substr(en->offset, en->length);
                    linkstore.save(url);
                    cout << "url: " << url << '\n';
                }

            }

        });
    TgLongPoll longPoll(bot);
    while(true) {
        try {
            clog << "Bot username: " << bot.getApi().getMe()->username << '\n';

            while (true) {
                clog << "Long poll started\n";
                longPoll.start();
            }
        } catch (TgException &e) {
            cerr << "error: " << e.what();
        }
    }
}
