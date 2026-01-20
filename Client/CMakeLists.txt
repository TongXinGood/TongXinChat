# 1. 升级最低版本 (Qt6 需要 3.16+)
cmake_minimum_required(VERSION 3.16)

# 2. 项目名称
project(TongXinChat LANGUAGES CXX)

# --- VS 默认生成的调试配置 (保留即可) ---
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
# ------------------------------------

# 3. 【核心】开启 Qt 自动化 (必须有！否则信号槽失效)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 4. 设定 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 5. 寻找 Qt6 库
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Multimedia Network)

# 6. 添加可执行文件 (只能写一次！)
# 如果你已经把 TongXinChat.cpp 重命名为 main.cpp，请把下面的 "TongXinChat.cpp" 改成 "main.cpp"
add_executable(TongXinChat
    "main.cpp"
    "TongXinChat.h"
)

# 7. 链接库
target_link_libraries(TongXinChat PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Network
)

# 8. (可选) 如果不想在 Windows 运行时看到黑框框，解开下面这行的注释
# set_target_properties(TongXinChat PROPERTIES WIN32_EXECUTABLE ON)