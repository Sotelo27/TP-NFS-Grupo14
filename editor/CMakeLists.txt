target_sources(taller_editor
    PRIVATE
    # .cpp files
    main.cpp
        editor_menu.cpp
    editor_choose_map.cpp
    editor_window.cpp
        clickable_image.cpp
    PUBLIC
    # .h files
        editor_menu.h
    editor_choose_map.h
    editor_window.h
        clickable_image.h
    )
