----------------------------------------------------------------------------------
-- Company: 		Letourneau University
-- Engineer: 		Samuel Armstrong
-- 
-- Create Date:   13:01:49 02/18/2025 
-- Design Name: 	Single Board Computer
-- Module Name:   main - Behavioral 
-- Project Name: 	Microcomputer Design
-- Target Devices:My Very own single board computer
-- Tool versions: ISE Design Suite 14.7
-- Description: 	The main code for the cpld
--
-- Dependencies: 	None
--
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity main is
    Port ( CPU_LDS : in  STD_LOGIC;
           CPU_UDS : in  STD_LOGIC;
           RAM_OE : out  STD_LOGIC;
           RAM_WE : out  STD_LOGIC;
           RAM_CE : out  STD_LOGIC;
           CLK : in  STD_LOGIC;
           R_BTN : in  STD_LOGIC;
           C_EN : out  STD_LOGIC;
           CPU_DTACK : out  STD_LOGIC;	
           UTP_TXE : in  STD_LOGIC;
           UTP_RXF : in  STD_LOGIC;
           CPU_R_W : in  STD_LOGIC;
           UTP_SIWU : out  STD_LOGIC;
           D0 : inout  STD_LOGIC;
           ROM_1_OE : out  STD_LOGIC;
           ROM_0_OE : out  STD_LOGIC;
           ROM_1_CE : out  STD_LOGIC;
           ROM_0_CE : out  STD_LOGIC;
           CPU_AS : in  STD_LOGIC;
           CPU_IPL2 : out  STD_LOGIC;		
           CPU_IPL1 : out  STD_LOGIC;		
           CPU_IPL0 : out  STD_LOGIC;		
           CPU_HALT : inout  STD_LOGIC;
           CPU_RST : inout  STD_LOGIC;
           CPU_FC2 : in  STD_LOGIC;			
           CPU_FC1 : in  STD_LOGIC;			
           CPU_FC0 : in  STD_LOGIC;			
           A : in  STD_LOGIC_VECTOR (20 downto 19);
           UTP_WR : out  STD_LOGIC;
           UTP_RD : out  STD_LOGIC;
           UTP_RST : out  STD_LOGIC;
           UTP_CBUS5 : inout  STD_LOGIC;	-- Unused
           UTP_CBUS6 : inout  STD_LOGIC);	-- Unused
end main;

architecture Behavioral of main is
	signal ROM_L_enabled : std_logic;
	signal ROM_H_enabled : std_logic;
	signal RAM_enabled: std_logic;
	signal UTP_enabled: std_logic;
	signal POLL_enabled: std_logic;
	signal set_dtack_1: std_logic;
	signal set_dtack_2: std_logic;
	signal encat: std_logic_vector(0 to 4);
	signal devSel: std_logic_vector(1 downto 0);
	
	-- For FSM
	type state_type is (ST0, ST1, ST2, ST3);
	-- For Read Interrupt FSM
	signal HI_PS, HI_NS : state_type;
	signal vec_num_read: std_logic;
	-- For Read Poll
	signal RC_PS, RC_NS : state_type;
	-- For Read UTP
	signal RD_PS, RD_NS : state_type;
	-- For Write Cycle FSM
	signal WC_PS, WC_NS : state_type;
	--signal vec_num_write: std_logic;
begin	
	-- Static Pins
	C_EN <= '1';
	
	CPU_DTACK <= '0' when (CPU_AS='0' OR set_dtack_1='1' OR set_dtack_2='1') else '1'; -- Pulled:  AND ((CPU_R_W = '1' AND encat /= "0000") OR (CPU_R_W = '0' AND (CPU_LDS = '0' OR CPU_UDS = '0')))
	UTP_RST <= '1';
	UTP_SIWU <= '1';
	
	CPU_IPL1 <= '1';
	CPU_IPL2 <= '1';
	
	-- Busses
	encat <= ROM_L_enabled & ROM_H_enabled & RAM_enabled & UTP_enabled & POLL_enabled;
	devSel <= A(20) & A(19);
	
	-- Reset
	CPU_RST <= R_BTN;
	CPU_HALT <= R_BTN;
	
	-- Peripheral selection from A [20 19]
	-- [0 0] ROM (ROM_0 is upper byte, ROM_1 is low byte)
	-- [0 1] RAM
	-- [1 0] UTP			(odd values)
	-- [1 1] UTP Poll		(odd values)
	
	ROM_L_enabled <= '1' when (devSel="00" AND CPU_LDS='0' AND CPU_R_W='1' AND CPU_AS='0') else '0';
	ROM_H_enabled <= '1' when (devSel="00" AND CPU_UDS='0' AND CPU_R_W='1' AND CPU_AS='0') else '0';
	with encat select ROM_0_CE <=
		'0' when "01000",
		'0' when "11000",
		'1' when others;
	with encat select ROM_0_OE <=
		'0' when "01000",
		'0' when "11000",
		'1' when others;
	with encat select ROM_1_CE <=
		'0' when "10000",
		'0' when "11000",
		'1' when others;
	with encat select ROM_1_OE <=
		'0' when "10000",
		'0' when "11000",
		'1' when others;
	
	
	RAM_enabled <= '1' when (devSel="01" AND CPU_AS='0') else '0';
	with encat select RAM_CE <=
		'0' when "00100",
		'Z' when others;
	with encat select RAM_OE <=
		'0' when "00100",
		'Z' when others;
	with CPU_R_W select RAM_WE <=
		'0' when '0',
		'Z' when others;
	
	-- Notes: 	Even addresses are considered upper, 
	--				Upper is considered D8-15
	--				CPU_LDS must be asserted to pass data to $100001
	--				CPU has glitch on write that copies data to both upper and lower
	UTP_enabled <= '1' when ((devSel="10" AND CPU_AS='0') OR vec_num_read='1') else '0';
	
	sync_read: process (clk, RD_NS)
	begin
		if (rising_edge(clk)) then
			RD_PS <= RD_NS;
		end if;
	end process sync_read;
	
	read_utp: process (RD_PS, CPU_R_W, encat, vec_num_read, UTP_RXF, CPU_LDS)
	begin
		case RD_PS is
			when ST0 =>
				set_dtack_1 <= '0';
				UTP_RD <= '1';
				if (((CPU_R_W='1' AND encat="00010") OR vec_num_read = '1') AND UTP_RXF = '0') then
					RD_NS <= ST1;
				end if;
			when ST1 =>
				set_dtack_1 <= '1';
				UTP_RD <= '0';
				if (CPU_LDS='1') then
					UTP_RD <= '1';
					set_dtack_1 <= '0';
					RD_NS <= ST0;
				end if;
			when others =>
				RD_NS <= ST0;
				UTP_RD <= '1';
				set_dtack_1 <= '0';
		end case;
	end process read_utp;
	
	-- UTP Poll
	POLL_enabled <= '1' when (devsel="11" and CPU_AS='0' AND CPU_R_W='1') else '0';
	
	sync_poll: process (clk, RC_NS)
	begin
		if (rising_edge(clk)) then
			RC_PS <= RC_NS;
		end if;
	end process sync_poll;
	
	read_poll: process (UTP_RXF, RC_PS, encat, CPU_LDS)
	begin
		case RC_PS is
			when ST0 =>
				D0 <= 'Z';
				set_dtack_2 <= '0';
				if (encat="00001" AND CPU_LDS='0') then
					RC_NS <= ST1;
				end if;
			when ST1 =>
				D0 <= UTP_RXF;
				set_dtack_2 <= '1';
				if (CPU_LDS='1') then
					D0 <= 'Z';
					set_dtack_2 <= '0';
					RC_NS <= ST0;
				end if;
			when others =>
				RC_NS <= ST0;
				D0 <= 'Z';
				set_dtack_2 <= '0';
		end case;
	end process read_poll;
	
	
	-- UTP Write
	-- Prevent double characters due to trace glitches
	sync_write: process (clk, WC_NS)
	begin
		if (rising_edge(clk)) then
			WC_PS <= WC_NS;
		end if;
		--if (UTP_RXF = '1') then
		--	HI_PS <= ST0;
		--end if;
	end process sync_write;
	
	write_cycle: process (WC_PS, CPU_R_W, encat, UTP_TXE, vec_num_read, CPU_LDS, CPU_AS)
	begin
		case WC_PS is
			when ST0 =>
				UTP_WR <= '1';
				--vec_num_write <= '0';
				if (CPU_R_W='0' AND encat="00010" AND UTP_TXE='0' AND vec_num_read='0' AND CPU_LDS='0') then
					WC_NS <= ST1;
				end if;
			when ST1 =>
				UTP_WR <= '0';
				--vec_num_write <= '1';
				if (UTP_TXE='1' or CPU_AS='1') then
					WC_NS <= ST2;
				end if;
			when ST2 =>
				UTP_WR <= '1';
				--vec_num_write <= '0';
				if (CPU_AS='1') then
					WC_NS <= ST0;
				end if;
			when others =>
				WC_NS <= ST0;
				UTP_WR <= '1';
		end case;
	end process write_cycle;
	
	
    	-- UTP Interrupt
    	-- * Device sinks IPL, indicates priority (1-7)
    	-- * MPU -> FC all high = interrupt, A1-3 = level
    	-- * Device sends "interrupt vector number" on data bus. (CHECK TABLE FOR USABLE NUMBERS pg 448)
    	-- *** Faking requires usb sends "ivn" as first byte for every interrupting message
    	-- *** CPLD loads calls a read on usb at this time *AND* asserts DTACK
    	-- ***** USB read when RD pin goes low
    	-- * MPU uses "ivn" to find handler routine
    	-- Currently cannot recieve data, just start an interrupt
	
	sync_interrupt: process (clk, HI_NS, UTP_RXF)
	begin
		if (rising_edge(clk)) then
			HI_PS <= HI_NS;
		end if;
		--if (UTP_RXF = '1') then
		--	HI_PS <= ST0;
		--end if;
	end process sync_interrupt;
	
	handle_interrupt: process(HI_PS, UTP_RXF, CPU_FC0, CPU_FC1, CPU_FC2, CPU_AS, CPU_LDS, WC_PS)
		
	begin
		case HI_PS is
			when ST0 =>
				CPU_IPL0 <= '1';
				vec_num_read <= '0';
				if (UTP_RXF = '0') then
					HI_NS <= ST1;
				end if;
			when ST1 =>
				CPU_IPL0 <= '0';
				vec_num_read <= '0';
				if (CPU_FC0 /= '0' AND CPU_FC1 /= '0' AND CPU_FC2 /= '0' AND CPU_AS = '0' AND CPU_LDS = '0') then
					HI_NS <= ST2;
				end if;
			when ST2 =>
				CPU_IPL0 <= '0';
				vec_num_read <= '1';
				if (CPU_AS = '1' AND CPU_LDS = '1') then
					HI_NS <= ST3;
				end if;
			when ST3 =>
				CPU_IPL0 <= '1';
				vec_num_read <= '0';
				if (WC_PS = ST1 OR R_BTN = '0') then
					HI_NS <= ST0;
				end if;
			end case;
	end process handle_interrupt;
 
	

end Behavioral;

