#include "http.h"

#include <Arduino.h>
#include <ArduinoLog.h>
#include "version.h"
#include "log.h"

Http::Http(GateSystem *system) : s(system)
{
}

void Http::setup() 
{
    Log.noticeln("Http       # Configuring webserver...");
    
    server.on("/", [&]() {
        render_home(s->state());
    });

    server.on("/gate_open", [&]() {
        Log.noticeln("Http        # ===========> Gate OPEN request received");
        s->open_gate();
        render_home(s->state());
    });

    server.on("/gate_close", [&]() {
        Log.noticeln("Http        # ===========> Gate CLOSE request received");
        s->close_gate();
        render_home(s->state());
    });

    server.on("/gate_stop", [&]() {
        Log.noticeln("Http        # ===========> Gate STOP request received");
        s->stop_gate();
        render_home(s->state());
    });

    server.on("/contractor_on", [&]() {
        Log.noticeln("Http        # CONTRACTOR ON request received");
        s->enabled_contactor();
        render_home(s->state());
    });

    server.on("/contractor_off", [&]() {
        Log.noticeln("Http        # CONTRACTOR OFF request received");
        s->disabled_contactor();
        render_home(s->state());
    });

    server.begin();

    Log.noticeln("Http       # HTTP server configured");
}

void Http::loop() 
{
  server.handleClient();
}

void Http::render_home(const GateState &s)
{

    static String header_str = "<!DOCTYPE html><html><head><title>Gate door controller, version: " + version_str + " </title></head><body>";
    static String footer_str = "</body></html>";
    
    static String form = \
    "<form action=\"/gate_open\" method=\"get\" id=\"form1\"></form> \
    <button type=\"submit\" form=\"form1\" value=\"Open gate\">Open gate</button> \
    <form action=\"/gate_stop\" method=\"get\" id=\"form_stop\"></form> \
    <button type=\"submit\" form=\"form_stop\" value=\"Stop gate\">Stop gate</button> \
    <form action=\"/gate_close\" method=\"get\" id=\"form2\"></form> \
    <button type=\"submit\" form=\"form2\" value=\"Close gate\">Close gate</button> \
    <form action=\"/contractor_on\" method=\"get\" id=\"form3\"></form> \
    <button type=\"submit\" form=\"form3\" value=\"Contractor on\">Contractor on</button> \
    <form action=\"/contractor_off\" method=\"get\" id=\"form4\"></form> \
    <button type=\"submit\" form=\"form4\" value=\"Contractor off\">Contractor off</button>";

    String gate_state_str = "<p>Gate state: " + s.current_state_str() + "</p>";

   if (s.is_stopped()) {
        gate_state_str += "<p>GATE STOPPED</p>";
    }
    String contractor_state_str = s.is_contactor_enabled() ? "<p>Contractor on</p>" : "<p>Contractor off</p>";
    String check_count_str = "<p>open_check count: " + String(s.gate_open_check_count) + "</p><p>close_check count: " + String(s.gate_close_check_count) + "</p>";
    
    server.send(200, "text/html", header_str + gate_state_str + contractor_state_str + check_count_str + form + footer_str);
}