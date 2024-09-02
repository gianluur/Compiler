; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 6, ptr %x, align 4
  %x1 = load i32, ptr %x, align 4
  %cmptmp = icmp eq i32 %x1, 3
  br i1 %cmptmp, label %then, label %elseif

then:                                             ; preds = %entry
  %currentValue = load i32, ptr %x, align 4
  store i32 0, ptr %x, align 4
  br label %ifcont

elseif:                                           ; preds = %entry
  %x2 = load i32, ptr %x, align 4
  %cmptmp3 = icmp eq i32 %x2, 4
  br i1 %cmptmp3, label %elseif_then, label %elseif_next

elseif_then:                                      ; preds = %elseif
  %currentValue4 = load i32, ptr %x, align 4
  store i32 1, ptr %x, align 4
  br label %ifcont

elseif_next:                                      ; preds = %elseif
  %x5 = load i32, ptr %x, align 4
  %cmptmp6 = icmp eq i32 %x5, 6
  br i1 %cmptmp6, label %elseif_then7, label %else

elseif_then7:                                     ; preds = %elseif_next
  %currentValue8 = load i32, ptr %x, align 4
  store i32 3, ptr %x, align 4
  br label %ifcont

else:                                             ; preds = %elseif_next
  %currentValue9 = load i32, ptr %x, align 4
  store i32 2, ptr %x, align 4
  br label %ifcont

ifcont:                                           ; preds = %else, %elseif_then7, %elseif_then, %then
  %x10 = load i32, ptr %x, align 4
  ret i32 %x10
}
