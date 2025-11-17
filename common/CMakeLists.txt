target_sources(taller_common
    PRIVATE
    # .cpp files
    liberror.cpp
    resolver.cpp
    resolvererror.cpp
    socket.cpp
    foo.cpp
    constant_rate_loop.cpp
    constant_rate_loop_server.cpp
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
    car_model.h
    constant_rate_loop.h
    constant_rate_loop_server.h
    # --- nuevos DTO usados por los métodos de ServerProtocol ---
    dto/car_info.h
    dto/results_info.h
    dto/map_tick_info.h
    )
