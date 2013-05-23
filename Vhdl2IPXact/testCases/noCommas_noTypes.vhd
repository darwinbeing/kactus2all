-- Not to be used in any design, only for testing purposes!
-- This test case presents ports and generics with wrong syntax

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned."+";
use IEEE.std_logic_arith.conv_unsigned;

entity foo is
	generic (
		  local_memory_start_addr : integer := 16#1000#;    -- upper 16 bits of address
		  local_memory_addr_bits  : integer := 12        -- number of address bits
		  code_file               : 		:= "master.tbl"; -- file to read commands from
		  width	: integer := 8; -- leveys
	);
	port (
		clk: in std_logic;
		addr: in std_logic_vector(11 downto 0)
		data: out std_logic_vector(31 downto 0);
		data2 : out 
		data_3 : in;
		intr: out std_logic;
		ip_clk: in std_logic;
		rst_an: in std_logic
		\sclIn\: in std_logic
		\sdaIn\: in std_logic;
		\sclOut\: out std_logic;
		\sdaOut\: out std_logic
	);
end foo;
