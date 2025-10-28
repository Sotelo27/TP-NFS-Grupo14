target_sources(taller_client
    PRIVATE
    # .cpp files
    server_handler.cpp
    server_thread_recv.cpp
    server_thread_send.cpp
    PUBLIC
    # .h files
    server_handler.h
    server_thread_recv.h
    server_thread_send.h
    )
