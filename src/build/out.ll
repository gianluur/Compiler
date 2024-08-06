; ModuleID = 'MyModule'
source_filename = "MyModule"

@test = global i32 5

define i32 @main() {
entry:
  %test = load i32, ptr @test, align 4
  %addtmp = add i32 %test, 1
  ret i32 %addtmp
}
