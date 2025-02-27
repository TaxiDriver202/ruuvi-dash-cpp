#include "crow.h"
#include <vector>
#include <unordered_set>

#define CROW_ENFORCE_WS_SPEC

static void send_ruuvi(std::string json, std::unordered_set<crow::websocket::connection*> usrs);



class 
tags
{
  private:
    uint_fast16_t i;
    struct 
    tag
    {
      std::string json;
      std::string id;
    };
  

  public:
    std::vector<tag> tag_data;
    std::string tag_json;

    std::unordered_map<std::string, std::string> tag_id;
  

    tags(void){}
    tags(crow::json::rvalue json)
    {
      i=0; 
      /* Create new tag struct and append it to the tag_data vector */
      for(auto data:json["data"]["tags"]){
        tag newtag;
        newtag.id = data["id"].s();
        
        std::ostringstream txt;
        /* << operator of class ostream, which places the data in the correct json format into the std::ostringstream object nice and easily */
        txt << "\"" << i << "\"" << ": " << "{\"temperature\": " << data["temperature"].d() << ",\"humidity\": " << data["humidity"].d() << ",\"pressure\": " << data["pressure"].d() << "}";
        newtag.json = txt.str();

        tag_id[newtag.id] = newtag.json;
        tag_data.push_back(newtag);
        i++;
      }


      /* create full json object */
      std::ostringstream h;
      h << "{" ;
      for(auto i =0; i<tag_data.size(); ++i){
        if(i == (tag_data.size()-1))
          {h << tag_data[i].json; break;}
        h << tag_data[i].json << ", ";
      }
      h << "}";
      tag_json = h.str();
    }
    
    tag operator[] (int i) {return tag_data[i];}
    void clear(void){tag_json.clear(); tag_data.clear();}
  
};


static std::unordered_set<crow::websocket::connection*> users;
static tags rtags = tags();


/* sends ruuvi data to all websocket connections */
void
send_ruuvi(std::string json, std::unordered_set<crow::websocket::connection*> usrs)
{
  if(!usrs.empty()) {
    for(auto u:usrs) u->send_text(json); 
  }
  else std::cout << "Users empty\n";
}



int 
main(void)
{
  
  crow::SimpleApp app;

  /* Serve Dashboard HTML */
  CROW_ROUTE(app, "/")([]()
  {
    auto dash = crow::mustache::load("dashboard.html");
    return dash.render();
  });

 
  /* POST request route where the Ruuvi Gateway sends its data */ 
  CROW_ROUTE(app, "/request")
  .methods("POST"_method)
  ([](const crow::request& req)
  {
    /* Read "raw" json strings into a json object */
    crow::json::rvalue data = crow::json::load(req.body); 

    /* If data is empty (e.g. GET request) return 400 */
    if(!data)
      return crow::response(400);
    
    /* Clear the rtag object if number of tags changes */
    if (data["data"]["tags"].keys().size() != rtags.tag_data.size()) 
        rtags.clear();

    /* update class using constructor which processes json data */
    rtags = tags(data); 
    
    /* send the new data to connected users */
    send_ruuvi(rtags.tag_json, users); 
   
    return crow::response{req.body};
  });



  /*
    WS route: insert pointer to the connection into the users set, and send existing data immediately.
    Erase the user when the connection is closed.
  */
  CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&] (crow::websocket::connection& conn)
      {
        users.insert(&conn);
        send_ruuvi(rtags.tag_json, users);
      })
      .onclose([&] (crow::websocket::connection& conn, const std::string& reason, uint16_t){
        users.erase(&conn);
      })
      .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){});
  
  app.port(18080).multithreaded().run();
}
