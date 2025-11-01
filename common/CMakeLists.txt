target_sources(taller_common
    PRIVATE
    # .cpp files
    liberror.cpp
    resolver.cpp
    resolvererror.cpp
    socket.cpp
    foo.cpp
    PUBLIC
    # .h files
    constants.h
    liberror.h
    queue.h
    resolver.h
    resolvererror.h
    socket.h
    thread.h
    foo.h
    # DTOs (reemplazan base_protocol.h)
    dto/movement.h
    dto/room_info.h
    dto/client_msg.h
    dto/server_msg.h
    )
