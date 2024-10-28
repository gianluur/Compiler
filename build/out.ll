; ModuleID = 'MyModule'
source_filename = "MyModule"

%Test = type { i32, float }

define i32 @main() {
entry:
  %x = alloca float, align 4
  store float 0xC0515AE140000000, ptr %x, align 4
  %test = alloca %Test, align 8
  %Test_instance = alloca %Test, align 8
  %member_ptr = getelementptr inbounds %Test, ptr %Test_instance, i32 0, i32 0
  store i32 2, ptr %member_ptr, align 4
  %member_ptr1 = getelementptr inbounds %Test, ptr %Test_instance, i32 0, i32 1
  store float 3.500000e+00, ptr %member_ptr1, align 4
  br label %for.init

for.init:                                         ; preds = %entry
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.update, %for.init
  %memberPtr = getelementptr inbounds %Test, ptr %Test_instance, i32 0, i32 0
  %memberValue = load i32, ptr %memberPtr, align 4
  %cmptmp = icmp slt i32 %memberValue, 10
  br i1 %cmptmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %memberPtr2 = getelementptr inbounds %Test, ptr %Test_instance, i32 0, i32 0
  %memberPtr3 = getelementptr inbounds %Test, ptr %Test_instance, i32 0, i32 0
  %memberValue4 = load i32, ptr %memberPtr3, align 4
  %addtmp = add i32 %memberValue4, 1
  store i32 %addtmp, ptr %memberPtr2, align 4
  br label %for.update

for.update:                                       ; preds = %for.body
  %currentValue = load i32, ptr %i, align 4
  %addtmp5 = add i32 %currentValue, 1
  store i32 %addtmp5, ptr %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret i32 0
}
