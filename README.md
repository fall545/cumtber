# cumtber
parser
说明:
getNextToken函数执行后会把token写入CurTok
getNextToken是会消耗token的，如果想要跳过 token也可以调用函数来实现

要求:

所有的.cpp放到src目录下,.h放到include目录中
例如
project_root/
├── CMakeLists.txt
├── main.cpp
├── src/
│   ├── foo.cpp
│   ├── bar.cpp
├── include/
│   ├── foo.h
│   ├── bar.h

文件命名规范
写parser的时候要单独新建一个.cpp的文件,如要完成expression的parser那就新建expressionast.cpp
不同的parser要放到不同的.cpp中,并把对外开放的接口函数原型写到同名的.h中如expressionast.h

函数的本体要写到.cpp中,声明必须统一放到同名.h文件中,方便调用

变量命名不做规范了


注意
若添加了新的文件一定要重新编译
cmake ..
make

TODO:
恺哥 完成函数
振宇 expression
宋 完成循环 

编译方法:
cd 项目所在根目录
mkdir build
cd build
cmake ..
make