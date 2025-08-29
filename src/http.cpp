#include "http.h"

#include <Arduino.h>
#include <ArduinoLog.h>  // If you are using the ArduinoLog library
#include "version.h"     // Contains version_str
#include "log.h"         // Contains your Log definitions (Log.noticeln, etc.)

// -- Hard-coded credentials for Basic Auth
static const char* HTTP_USERNAME = "admin";
static const char* HTTP_PASSWORD = "wospamdk6";

Http::Http(GateSystem* system) : s(system) {}

void Http::setup()
{
    Log.noticeln("Http       # Configuring webserver...");

    // Map each route to a handler
    server.on("/", HTTP_GET, [this]() {
        if (!requireAuth()) return;
        handleRoot();
    });

    server.on("/gate_open", HTTP_GET, [this]() {
        if (!requireAuth()) return;
        handleGateOpen();
    });

    server.on("/gate_close", HTTP_GET, [this]() {
        if (!requireAuth()) return;
        handleGateClose();
    });

    server.on("/gate_stop", HTTP_GET, [this]() {
        if (!requireAuth()) return;
        handleGateStop();
    });

    // Catch-all for 404 Not Found
    server.onNotFound([this]() {
        if (!requireAuth()) return;
        server.send(404, "text/plain", "Not found");
    });

    // Start the server
    server.begin();
    Log.noticeln("Http       # HTTP server configured");
}

void Http::loop()
{
    server.handleClient();
}

// -------------------------------------------------
// Authentication helper
// -------------------------------------------------
bool Http::requireAuth()
{
    // If not authenticated, prompt for credentials
    if (!server.authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
        server.requestAuthentication();  // Sends 401 Unauthorized with Basic Auth header
        return false;
    }
    return true;
}

// -------------------------------------------------
// Route Handlers
// -------------------------------------------------
void Http::handleRoot()
{
    renderHome();
}

void Http::handleGateOpen()
{
    Log.noticeln("Http       # ===========> Gate OPEN request received");
    s->open_gate();
    renderHome();
}

void Http::handleGateClose()
{
    Log.noticeln("Http       # ===========> Gate CLOSE request received");
    s->close_gate();
    renderHome();
}

void Http::handleGateStop()
{
    Log.noticeln("Http       # ===========> Gate STOP request received");
    s->stop_gate();
    renderHome();
}

// -------------------------------------------------
// HTML Rendering (Responsive / Mobile-Friendly)
// -------------------------------------------------
void Http::renderHome()
{
    // Using a raw string literal for HTML markup
    String page = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <!-- Responsive viewport for mobile-friendly design -->
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <title>Gate Door Controller
)";

    // Append version to the <title>
    page += ", version: ";
    page += version_str;
    page += R"(</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            margin: 0;
            padding: 0;
        }
        .container {
            max-width: 600px;  /* Will shrink automatically on smaller screens */
            margin: 50px auto;
            background-color: #fff;
            padding: 20px;
            border-radius: 6px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.2);
        }
        h1 {
            text-align: center;
            margin-top: 0;
        }
        .status {
            font-size: 1.2em;
            color: #555;
            margin-bottom: 20px;
            text-align: center;
        }
        .button-group {
            display: flex;
            flex-wrap: wrap;
            gap: 10px;
            justify-content: center;
        }
        .button-group form {
            margin: 0;
        }
        button {
            padding: 12px 20px;
            font-size: 1em;
            color: #fff;
            background-color: #007bff;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            white-space: nowrap;
        }
        button:hover {
            background-color: #0056b3;
        }
        footer {
            margin-top: 20px;
            text-align: center;
            color: #999;
            font-size: 0.9em;
        }
        /* Media query to stack buttons on smaller screens */
        @media (max-width: 600px) {
            .button-group {
                flex-direction: column;
                align-items: center;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Gate Door Controller</h1>
)";

    // Show the gate status
    page += "<div class=\"status\">Gate state: " + s->get_gate_state().current_state_str() + "</div>";

    // If gate is stopped
    if (s->get_gate_state().is_stopped()) {
        page += "<div class=\"status\" style=\"color:red;\">GATE STOPPED</div>";
    }

    // Show contractor state
    if (s->get_gate_state().is_contactron_enabled()) {
        page += "<div class=\"status\">Contactron: ON</div>";
    } else {
        page += "<div class=\"status\">Contactron: OFF</div>";
    }

    // Buttons
    page += R"(
        <div class="button-group">
            <form action="/gate_open" method="get">
                <button>Open Gate</button>
            </form>
            <form action="/gate_stop" method="get">
                <button>Stop Gate</button>
            </form>
            <form action="/gate_close" method="get">
                <button>Close Gate</button>
            </form>
        </div>
    </div>
    <footer>
        Gate Door Controller v)";

    page += version_str;
    page += R"(
    </footer>
</body>
</html>
)";

    server.send(200, "text/html", page);
}
