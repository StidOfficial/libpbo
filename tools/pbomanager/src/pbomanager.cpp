#include "PBOManagerWindow.hpp"

#include <gtkmm/application.h>

int main(int argc, char **argv)
{
    auto app = Gtk::Application::create(argc, argv, "org.libpbo.pbomanager");

    PBOManagerWindow window;
    return app->run(window);
}