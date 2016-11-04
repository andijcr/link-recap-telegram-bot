#include <iostream>
#include <vector>
#include <regex>

#include <tgbot/tgbot.h>

#include <boost/property_tree/ini_parser.hpp>
#include <boost/network/uri.hpp>

using namespace std;
using namespace TgBot;
using namespace boost::property_tree;
using namespace boost::network;

template<typename MessageType, typename = enable_if_t<is_copy_constructible<MessageType>::value>>
class LinkBin{
    vector<MessageType> to_save{};
public:
    void save(MessageType m){
        to_save.push_back(move(m));
    }

    vector<MessageType> recap(){
        vector<MessageType> res;
        swap(to_save, res);
        return res;
    }


};


#pragma clang diagnostic push
#pragma ide diagnostic ignored "IncompatibleTypes"
int main(){

    //this catch some symbols with a point in the middle, basically
    //R"reg([^\s]+\.[\w]{2,})reg";

    LinkBin<string> linkstore;
    regex url_like;
    string api_telegram_token;
    try {
        ptree pt;
        json_parser::read_json("link_recap_bot.config", pt);
        api_telegram_token = pt.get<string>("token");
        url_like=regex(pt.get<string>("url_regex"));

    } catch(exception& e){
        cerr << "error while reading configuration: " << e.what() << '\n';
        throw;
    }

    Bot bot(api_telegram_token);

    bot.getEvents().onCommand("recap", [&](auto message) {
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

    bot.getEvents().onNonCommandMessage([&](auto message) {
        cout << "User wrote: " << message->text << '\n';

        smatch url_res;
        regex_search(message->text, url_res, url_like);
        cout << "is there an url?: " <<  !url_res.empty() << '\n';
        for(auto u: url_res){
            cout << "url: " << u << '\n';
            linkstore.save(u);
        }

//bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
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
    return 0;
}
#pragma clang diagnostic pop