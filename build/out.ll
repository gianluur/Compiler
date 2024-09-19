; ModuleID = 'MyModule'
source_filename = "MyModule"

%Test = type { i32, float }

define i32 @main() {
entry:
  %test = alloca %Test, align 8
  %Test_instance = alloca %Test, align 8
  %member_ptr = getelementptr inbounds %Test, ptr %Test_instance, i32 0, i32 0
  store i32 2, ptr %member_ptr, align 4
  %member_ptr1 = getelementptr inbounds %Test, ptr %Test_instance, i32 0, i32 1
  store float 3.500000e+00, ptr %member_ptr1, align 4
  %memberPtr = getelementptr inbounds %Test, ptr %Test_instance, i32 0, i32 0
  %memberValue = load i32, ptr %memberPtr, align 4
  %addtmp = add i32 %memberValue, 4
  ret i32 %addtmp
}
