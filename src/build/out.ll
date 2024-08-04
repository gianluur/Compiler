; ModuleID = 'MyModule'
source_filename = "MyModule"

@test = global i8 65

define i32 @main() {
entry:
  %test = load i8, ptr @test, align 1
  %charToInt = zext i8 %test to i32
  ret i32 %charToInt
}
