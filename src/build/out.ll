; ModuleID = 'MyModule'
source_filename = "MyModule"

%Test = type { i32, float }

define i32 @main() {
entry:
  %test = alloca %Test, align 8
  %memberPtr = getelementptr inbounds %Test, ptr %test, i32 0, i32 0
  store i32 5, ptr %memberPtr, align 4
  %memberPtr1 = getelementptr inbounds %Test, ptr %test, i32 0, i32 1
  store float 3.500000e+00, ptr %memberPtr1, align 4
  %x = alloca i32, align 4
  %memberPtr2 = getelementptr inbounds %Test, ptr %test, i32 0, i32 0
  %memberValue = load i32, ptr %memberPtr2, align 4
  %memberPtr3 = getelementptr inbounds %Test, ptr %test, i32 0, i32 1
  %memberValue4 = load float, ptr %memberPtr3, align 4
  %floatToInt = fptosi float %memberValue4 to i32
  %addtmp = add i32 %memberValue, %floatToInt
  store i32 %addtmp, ptr %x, align 4
  %x5 = load i32, ptr %x, align 4
  ret i32 %x5
}
