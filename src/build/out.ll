; ModuleID = 'MyModule'
source_filename = "MyModule"

@test2 = global i32 10

define i32 @main() {
entry:
  %test = alloca i32, align 4
  store i32 5, ptr %test, align 4
  ret i32 0
}
