target_sources(taller_server
    PRIVATE
    # .cpp files
    main.cpp
    acceptor.cpp
    client_handler.cpp
    client_thread_recv.cpp
    client_thread_send.cpp
    client_list.cpp
    gameloop.cpp
    game.cpp
    race.cpp
    physics_world.cpp
    server.cpp
    server_protocol.cpp
    monitor_lobby.cpp
    city.cpp
    PUBLIC
    # .h files
    acceptor.h
    client_action.h
    client_handler.h
    client_thread_recv.h
    client_thread_send.h
    client_list.h
    gameloop.h
    game.h
    race.h
    city.h
    physics_world.h
    physics/Entidad.h
    race_participant.h
    server.h
    server_protocol.h
    monitor_lobby.h
    )

# Headers del sistema (Box2D está en /usr/include/box2d)
target_include_directories(taller_server PRIVATE /usr/include)

# Linkea la lib (libbox2d.so => "box2d")
target_link_libraries(taller_server box2d)
