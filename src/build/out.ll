; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 11, ptr %x, align 4
  br label %dowhile.body

dowhile.body:                                     ; preds = %dowhile.cond, %entry
  %currentValue = load i32, ptr %x, align 4
  %addtmp = add i32 %currentValue, 1
  store i32 %addtmp, ptr %x, align 4
  br label %dowhile.cond

dowhile.cond:                                     ; preds = %dowhile.body
  %x1 = load i32, ptr %x, align 4
  %cmptmp = icmp slt i32 %x1, 10
  br i1 %cmptmp, label %dowhile.body, label %dowhile.end

dowhile.end:                                      ; preds = %dowhile.cond
  %x2 = load i32, ptr %x, align 4
  ret i32 %x2
}
