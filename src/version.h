#define VERSION_MAJOR 1
#define VERSION_MINOR 3

static String version_str = String(VERSION_MAJOR) + "." + String(VERSION_MINOR) + " (build date: " + str(__DATE__) + " " + str(__TIME__) + ")";
