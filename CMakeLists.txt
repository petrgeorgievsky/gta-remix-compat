cmake_minimum_required(VERSION 3.18)
project("gta-remix-compat")

find_package(wil CONFIG REQUIRED) # Windows utility helpers

add_subdirectory("remix_compat")

install(TARGETS remix_compat DESTINATION .)
