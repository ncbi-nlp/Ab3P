import ctypes

ab3p = ctypes.CDLL("./wrapper.so")  # Dynamic-link library

ab3p.get_abbrevs_cpp.restype = None
ab3p.get_abbrevs_cpp.argstype = [ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint)]

sample = ctypes.create_string_buffer(b"This Is Some (TIS) text with Multiple-Abbreviations (MA) in it")
output_buffer_default_size = 10
output_size = ctypes.pointer(ctypes.c_uint(output_buffer_default_size))

result = ctypes.create_string_buffer(b"", output_size.contents.value)
result3 = ctypes.create_string_buffer(b"", output_size.contents.value)
result4 = ctypes.create_string_buffer(b"", output_size.contents.value)

ab3p.get_abbrevs_cpp(sample, result, output_size)
print(result.raw)
print('content size first call', output_size.contents.value)
if output_size.contents.value > output_buffer_default_size: 
    result2 = ctypes.create_string_buffer(b"", output_size.contents.value + 1)
    print('size of buffer second', output_size.contents.value + 1)
    ab3p.get_abbrevs_cpp(sample, result2, output_size)
    called = True

print(result.raw)
print(result2.raw)

