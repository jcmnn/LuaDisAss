local localvar = "asd"

local function a(...)
  local args = table.concat({...}, ",")

  local function b(f, txt)
    return f(txt)
  end

  return b(print, args)
end

a("Hello", " world!")