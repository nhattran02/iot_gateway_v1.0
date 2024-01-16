#Add sources from ui directory
file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "gui/*c" "app/*c")

idf_component_register(SRCS "main.c" "gui_setting.c" ${SRC_UI}
                       INCLUDE_DIRS "." "gui" "app")
