; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @test() {
entry:
  ret i32 5
}

define i32 @main() {
entry:
  %test_call = call i32 @test()
  ret i32 0
}
