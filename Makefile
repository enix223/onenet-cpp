.PHONY: run config-debug config-release debug release run-debug

config-debug:
	cmake -DCMAKE_INSTALL_PREFIX=out/install/linux-debug \
		-DCMAKE_BUILD_TYPE=Debug \
		-S . \
		-B out/build/linux-debug

config-release:
	cmake -DCMAKE_INSTALL_PREFIX=out/install/linux-release \
		-DCMAKE_BUILD_TYPE=Release \
		-S . \
		-B out/build/linux-release

debug: config-debug
	cmake --build out/build/linux-debug --

release: config-relese
	cmake --build out/build/linux-release --

run:
	export $$(cat .env | xargs) && \
		./out/build/linux-debug/onenet -p $$PRODUCT_ID -s $$PRODUCT_SECRET -d $$DEVICE_NAME -t $$DEVICE_SECRET -a $$DEVICE_LEVEL_AUTH

run-debug:
	export $$(cat .env | xargs) && \
		gdb --args ./out/build/linux-debug/onenet -p $$PRODUCT_ID -s $$PRODUCT_SECRET -d $$DEVICE_NAME -t $$DEVICE_SECRET -a $$DEVICE_LEVEL_AUTH
