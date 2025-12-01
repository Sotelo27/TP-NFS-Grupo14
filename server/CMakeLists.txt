target_sources(taller_server
    PRIVATE
    # .cpp files
    main.cpp
    acceptor.cpp
    client_handler.cpp
    client_thread_recv.cpp
    client_thread_send.cpp
    client_list.cpp
    server.cpp
    server_protocol.cpp
    monitor_lobby.cpp

    room_manager.cpp       
    pending_manager.cpp    
    binding_manager.cpp    

    PUBLIC
    # .h files
    acceptor.h
    client_action.h
    client_handler.h
    client_thread_recv.h
    client_thread_send.h
    client_list.h
    server.h
    server_protocol.h
    monitor_lobby.h

    room_manager.h         
    pending_manager.h      
    binding_manager.h      
)
