target_sources(taller_tests
    PRIVATE
    # .cpp files
    main_tests.cpp
    protocol_tests.cpp
    ../common/socket.cpp
    ../client/client_protocol.cpp
    ../server/client_handler.cpp
    ../server/server_protocol.cpp
    ../server/client_thread_recv.cpp
    ../server/client_thread_send.cpp
    PUBLIC
    # .h files
    protocol_tests.h
    ../common/socket.h
    ../client/client_protocol.h
    ../server/client_handler.h
    ../server/server_protocol.h
    ../server/client_thread_recv.h
    ../server/client_thread_send.h
    )
