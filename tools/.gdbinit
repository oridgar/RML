target remote localhost:1234
# Real mode
set architecture i8086
#set disassembly-flavor intel
b *0x7c00
b *0x2 if $cs == 0x1000
b startk
layout src
layout reg

define rm_print
	p/d (int)*((unsigned long)&$arg0 + ((unsigned long)($cs << 4)))
end

define rm_print_char
	p/d (char)*((unsigned long)&$arg0 + ((unsigned long)($cs << 4)))
end


set prompt (RML) 

set confirm off
set verbose off

define display_next_instruction
  display/i $pc + ($cs << 4)
end

define undisplay_next_instruction
  undisplay 1
end

display_next_instruction

define rm_step_until_iret
  undisplay_next_instruction
  set $_found = 0
  while (!$_found)
    if (*(unsigned char*)$eip == 0xCF)
      set $_found = 1
    else
      nexti
    end
  end
  display_next_instruction
end


