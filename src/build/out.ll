; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  %test = alloca i32, align 4
  store i32 5, ptr %test, align 4
  %test1 = load i32, ptr %test, align 4
  ret i32 %test1
}
