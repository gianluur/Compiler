; ModuleID = 'MyModule'
source_filename = "MyModule"

%Test = type { i32, float }

define i32 @main() {
entry:
  %test = alloca %Test, align 8
  ret i32 0
}
