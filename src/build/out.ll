; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @suca(i32 %0, i32 %1) {
entry:
  ret i32 0
}

define i32 @main() {
entry:
  %suca_call = call i32 @suca(i32 3, i32 1235)
  ret i32 %suca_call
}
