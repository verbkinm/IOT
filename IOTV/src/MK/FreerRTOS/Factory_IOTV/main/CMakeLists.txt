idf_component_register(SRCS "main.c"
						"global_extern.c"
						"wifi/wifi.c"
						"web/web.c"
						INCLUDE_DIRS "."
						EMBED_FILES "web/favicon.ico" "web/html/index.html" "web/html/connecting.html" "web/html/ota.html" "web/html/error.html")
