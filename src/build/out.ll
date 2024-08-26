; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 0, ptr %x, align 4
  %x1 = load i32, ptr %x, align 4
  ret i32 %x1
}
