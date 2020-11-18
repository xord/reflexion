# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflex'


class Win < Reflex::Window

  def initialize ()
    super title: "Camera Example", frame: [100, 100, 800, 600]
    @camera = Rays::Camera.new {start}
  end

  def on_draw (e)
    p = e.painter
    p.image @camera.image, 0, 0 if @camera.image
    p.text "#{e.fps.to_i} FPS", 10, 10
  end

  def on_update (e)
    redraw
  end

end

Reflex.start do
  Win.new.show
end
