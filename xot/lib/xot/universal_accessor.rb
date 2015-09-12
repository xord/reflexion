# -*- coding: utf-8 -*-


module Xot


  module UniversalAccessor

    def universal_accessor (*names, **options)
      names.each do |name|
        define_universal_accessor name, options
      end
    end

    private

      def define_universal_accessor (name, options)
        set = "ua_set_#{name}__"
        get = "ua_get_#{name}__"
        class_eval <<-END
          alias_method :#{set}, options[:writer] || :#{name}=
          alias_method :#{get}, options[:reader] || :#{name}
          private :#{set}, :#{get}
          def #{name} (*args)
            #{set} *args unless args.empty?
            #{get}
          end
        END
      end

  end# UniversalAccessor


end# Xot


class Module

  include Xot::UniversalAccessor

end# Module
