# -*- coding: utf-8 -*-


module Xot


  class BitFlag

    def initialize (none = 0, &block)
      @bit2sym, @sym2bit = {}, {none: none, no: none}
      BlockUtil.instance_eval_or_block_call self, &block if block
    end

    def flag (symbol, bit)
      @sym2bit[symbol] = bit
      @bit2sym[bit]    = symbol if bit.to_s(2).count('1') == 1
    end

    def bits2symbols (bits)
      array = []
      bits.to_s(2).reverse.each_char.with_index do |char, index|
        next unless char == '1'
        bit = BitFlag.bit index
        symbol = @bit2sym[bit]
        raise "unknown bit for flag." unless symbol
        array << symbol
      end
      array
    end

    def symbols2bits (*symbols)
      symbols.map {|sym| sym2bit sym}.reduce(0) {|a, b| a | b}
    end

    def self.bit (*args)
      bits = 0
      args.each {|n| bits |= 0x1 << n}
      bits
    end

    private

      def sym2bit (symbol)
        bit = @sym2bit[symbol]
        raise 'unknown symbol for flag.' unless bit
        bit
      end

  end# BitFlag


end# Xot
