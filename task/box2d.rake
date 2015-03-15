# -*- mode: ruby; coding: utf-8 -*-


namespace :box2d do


  mod          = MODULE
  box2d_srcdir = env :BOX2D, '~/lib/box2d/Box2D/Box2D'
  srcdir       = mod.root_dir 'src/physics'


  task :update => srcdir do
    sh %( rm -rf #{srcdir}/* )
    sh %( cp -r #{box2d_srcdir} #{srcdir} )

    Dir.glob "#{srcdir}/**/*" do |path|
      ext = path[/\.(.+?)$/]
      sh %( rm -f #{path} ) if ext && ext !~ /\.(h|cpp)/i
    end
  end

  directory srcdir


end# :box2d
