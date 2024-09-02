; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  %y = alloca i32, align 4
  store i32 0, ptr %y, align 4
  br label %for.init

for.init:                                         ; preds = %entry
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.update, %for.init
  %i1 = load i32, ptr %i, align 4
  %cmptmp = icmp slt i32 %i1, 10
  br i1 %cmptmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %i2 = load i32, ptr %i, align 4
  %addtmp = mul i32 %i2, 2
  %currentValue = load i32, ptr %y, align 4
  store i32 %addtmp, ptr %y, align 4
  br label %for.update

for.update:                                       ; preds = %for.body
  %currentValue3 = load i32, ptr %i, align 4
  %addtmp4 = add i32 %currentValue3, 1
  store i32 %addtmp4, ptr %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %x = alloca i32, align 4
  %y5 = load i32, ptr %y, align 4
  store i32 %y5, ptr %x, align 4
  %x6 = load i32, ptr %x, align 4
  ret i32 %x6
}
