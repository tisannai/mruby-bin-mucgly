MRuby::Gem::Specification.new('mruby-bin-mucgly') do |spec|
  spec.license = 'MIT'
  spec.author  = 'Tero Isannainen'
  spec.summary = 'mucgly command'

  spec.bins = %w(mucgly)

  spec.add_dependency( 'mruby-mucgly', '>= 0.0.0' )

  spec.cc.flags = [ENV['CFLAGS'] || %w(-std=gnu11)]

  glib_lib = %x{pkg-config --libs glib-2.0}[2..-2]
  glib_inc = %x{pkg-config --cflags glib-2.0}.split.map{|i| i[2..-1]}

  spec.cc.include_paths += glib_inc
  spec.linker.libraries << glib_lib

end
