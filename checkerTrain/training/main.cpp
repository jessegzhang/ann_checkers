#include "tournament.h"
#include <iostream>
#include <utility>
#include <map>
int main(int argc, char *argv[])
{

//    auto app =
//            Gtk::Application::create(argc, argv,
//                                     "org.gtkmm.examples.base");
//    Board board("rrrrrrrrrrrr________bbbbbbbbbbbb", 'r', true);
//    //Board board;
//    return app->run(board);
    for(int i=196; i<500000; i++) {
        Tournament checkTourney(i,50);
        checkTourney.playTournament();
    }


  //  return 0;
}
