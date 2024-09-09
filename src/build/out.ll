; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @test() {
entry:
  ret i32 5
}

define i32 @main() {
entry:
  %x = alloca i32, align 4
  %test_call = call i32 @test()
  %addtmp = add i32 3, %test_call
  %addtmp1 = add i32 %addtmp, 4
  store i32 %addtmp1, ptr %x, align 4
  %x2 = load i32, ptr %x, align 4
  ret i32 %x2
}
