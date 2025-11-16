target_sources(taller_client
        PRIVATE
        # .cpp files
        login_screen.cpp
        lobby_screen.cpp
        waiting_room_screen.cpp
        result_finish_screen.cpp
        selection_car_screen.cpp
        selection_map_screen.cpp
        game_window.cpp
        menu_screen.cpp
        PUBLIC
        # .h files
        login_screen.h
        lobby_screen.h
        waiting_room_screen.h
        result_finish_screen.h
        selection_car_screen.h
        selection_map_screen.h
        game_window.h
        menu_screen.h
)
