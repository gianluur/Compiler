; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 4, ptr %x, align 4
  %x1 = load i32, ptr %x, align 4
  %cmptmp = icmp eq i32 %x1, 5
  br i1 %cmptmp, label %then, label %else

then:                                             ; preds = %entry
  %currentValue = load i32, ptr %x, align 4
  store i32 0, ptr %x, align 4
  br label %ifcont

else:                                             ; preds = %entry
  %currentValue2 = load i32, ptr %x, align 4
  store i32 5, ptr %x, align 4
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %x3 = load i32, ptr %x, align 4
  ret i32 %x3
}
