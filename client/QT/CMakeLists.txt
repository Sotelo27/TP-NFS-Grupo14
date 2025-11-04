target_sources(taller_client
        PRIVATE
        # .cpp files
        login_window.cpp
        lobby_window.cpp
        waiting_room_window.cpp
        result_finish_window.cpp
        PUBLIC
        # .h files
        login_window.h
        lobby_window.h
        waiting_room_window.h
        result_finish_window.h
)
