; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 5, ptr %x, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %x1 = load i32, ptr %x, align 4
  %cmptmp = icmp slt i32 %x1, 10
  br i1 %cmptmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %currentValue = load i32, ptr %x, align 4
  %addtmp = add i32 %currentValue, 1
  store i32 %addtmp, ptr %x, align 4
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %x2 = load i32, ptr %x, align 4
  ret i32 %x2
}
