const textHue = 0x480;				// Color of the text.
const Blacksmithing = 4023;
const Carpentry = 4025;
const Alchemy = 4028;
const Fletching = 4029;
const Tailoring = 4030;
const scriptID = 4026; // This script
const exceptionalWearablesOnly = true;

function ItemDetailGump( pUser )
{
	var socket = pUser.socket;
	var itemGump = new Gump;
	var createEntry;
	var HARVEST;
	var mainSkill;
	switch( pUser.GetTempTag( "ITEMDETAILS" ))
	{
		//Start Blacksmith
		case 1:
			createEntry = CreateEntries[1];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 2:
			createEntry = CreateEntries[2];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 3:
			createEntry = CreateEntries[3];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 4:
			createEntry = CreateEntries[4];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 5:
			createEntry = CreateEntries[5];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 6:
			createEntry = CreateEntries[6];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 7:
			createEntry = CreateEntries[7];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 8:
			createEntry = CreateEntries[8];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 9:
			createEntry = CreateEntries[9];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 10:
			createEntry = CreateEntries[10];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 11:
			createEntry = CreateEntries[11];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 12:
			createEntry = CreateEntries[12];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 13:
			createEntry = CreateEntries[13];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 14:
			createEntry = CreateEntries[14];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 15:
			createEntry = CreateEntries[15];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 16:
			createEntry = CreateEntries[16];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 17:
			createEntry = CreateEntries[17];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 18:
			createEntry = CreateEntries[18];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 19:
			createEntry = CreateEntries[19];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 20:
			createEntry = CreateEntries[20];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 21:
			createEntry = CreateEntries[21];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 22:
			createEntry = CreateEntries[22];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 23:
			createEntry = CreateEntries[23];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 24:
			createEntry = CreateEntries[24];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 25:
			createEntry = CreateEntries[25];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 26:
			createEntry = CreateEntries[26];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 27:
			createEntry = CreateEntries[27];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 28:
			createEntry = CreateEntries[28];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 29:
			createEntry = CreateEntries[29];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 30:
			createEntry = CreateEntries[30];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 31:
			createEntry = CreateEntries[31];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 32:
			createEntry = CreateEntries[32];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 33:
			createEntry = CreateEntries[33];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 34:
			createEntry = CreateEntries[34];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 35:
			createEntry = CreateEntries[35];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 36:
			createEntry = CreateEntries[36];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 37:
			createEntry = CreateEntries[37];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 38:
			createEntry = CreateEntries[38];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 39:
			createEntry = CreateEntries[39];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 40:
			createEntry = CreateEntries[40];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 41:
			createEntry = CreateEntries[41];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 42:
			createEntry = CreateEntries[42];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 43:
			createEntry = CreateEntries[43];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 44:
			createEntry = CreateEntries[44];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 45:
			createEntry = CreateEntries[45];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 46:
			createEntry = CreateEntries[46];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 47:
			createEntry = CreateEntries[47];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 48:
			createEntry = CreateEntries[48];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 49:
			createEntry = CreateEntries[49];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		//Start Carpentry
		case 50:
			createEntry = CreateEntries[50];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 51:
			createEntry = CreateEntries[51];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 52:
			createEntry = CreateEntries[52];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 53:
			createEntry = CreateEntries[53];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 54:
			createEntry = CreateEntries[54];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 55:
			createEntry = CreateEntries[55];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 56:
			createEntry = CreateEntries[56];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 57:
			createEntry = CreateEntries[57];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 58:
			createEntry = CreateEntries[58];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 59:
			createEntry = CreateEntries[59];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 60:
			createEntry = CreateEntries[60];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 61:
			createEntry = CreateEntries[61];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 62:
			createEntry = CreateEntries[62];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 63:
			createEntry = CreateEntries[63];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 64:
			createEntry = CreateEntries[64];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 65:
			createEntry = CreateEntries[65];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 66:
			createEntry = CreateEntries[66];
			HARVEST = [10611, 10612];//missing loops
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 67:
			createEntry = CreateEntries[67];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 68:
			createEntry = CreateEntries[68];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 69:
			createEntry = CreateEntries[69];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 70:
			createEntry = CreateEntries[70];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 71:
			createEntry = CreateEntries[71];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 72:
			createEntry = CreateEntries[72];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 73:
			createEntry = CreateEntries[73];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 74:
			createEntry = CreateEntries[74];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 75:
			createEntry = CreateEntries[75];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 76:
			createEntry = CreateEntries[76];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 77:
			createEntry = CreateEntries[77];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 78:
			createEntry = CreateEntries[78];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 79:
			createEntry = CreateEntries[79];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 80:
			createEntry = CreateEntries[80];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 81:
			createEntry = CreateEntries[81];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 82:
			createEntry = CreateEntries[82];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 83:
			createEntry = CreateEntries[83];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 84:
			createEntry = CreateEntries[84];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 85:
			createEntry = CreateEntries[85];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 86:
			createEntry = CreateEntries[86];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 87:
			createEntry = CreateEntries[87];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 88:
			createEntry = CreateEntries[8];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 89:
			createEntry = CreateEntries[89];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 90:
			createEntry = CreateEntries[90];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 91:
			createEntry = CreateEntries[91];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 92:
			createEntry = CreateEntries[92];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 93:
			createEntry = CreateEntries[93];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 94:
			createEntry = CreateEntries[94];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 95:
			createEntry = CreateEntries[95];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 96:
			createEntry = CreateEntries[96];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 97:
			createEntry = CreateEntries[97];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 98:
			createEntry = CreateEntries[98];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 99:
			createEntry = CreateEntries[99];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 100:
			createEntry = CreateEntries[100];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 101:
			createEntry = CreateEntries[101];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 102:
			createEntry = CreateEntries[102];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 103:
			createEntry = CreateEntries[103];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 104:
			createEntry = CreateEntries[104];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 105:
			createEntry = CreateEntries[105];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 106:
			createEntry = CreateEntries[106];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 107:
			createEntry = CreateEntries[107];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 108:
			createEntry = CreateEntries[108];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 109:
			createEntry = CreateEntries[109];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 110:
			pUser.TextMessage( "Case 110!" );
			createEntry = CreateEntries[110];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 111:
			createEntry = CreateEntries[111];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 112:
			createEntry = CreateEntries[112];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 113:
			createEntry = CreateEntries[113];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 114:
			createEntry = CreateEntries[114];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 115:
			createEntry = CreateEntries[115];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 116:
			createEntry = CreateEntries[116];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 117:
			createEntry = CreateEntries[117];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 118:
			createEntry = CreateEntries[118];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 119:
			createEntry = CreateEntries[119];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 120:
			createEntry = CreateEntries[120];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 121:
			createEntry = CreateEntries[121];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 122:
			createEntry = CreateEntries[122];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 123:
			createEntry = CreateEntries[123];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 124:
			createEntry = CreateEntries[124];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		// Start Tailoring
		case 130:
			createEntry = CreateEntries[130];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 131:
			createEntry = CreateEntries[131];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 132:
			createEntry = CreateEntries[132];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 133:
			createEntry = CreateEntries[133];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 134:
			createEntry = CreateEntries[134];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 135:
			createEntry = CreateEntries[135];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 136:
			createEntry = CreateEntries[136];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 137:
			createEntry = CreateEntries[137];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 138:
			createEntry = CreateEntries[138];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 139:
			createEntry = CreateEntries[139];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 140:
			createEntry = CreateEntries[140];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 141:
			createEntry = CreateEntries[141];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 142:
			createEntry = CreateEntries[142];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 143:
			createEntry = CreateEntries[143];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 144:
			createEntry = CreateEntries[144];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 145:
			createEntry = CreateEntries[145];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 146:
			createEntry = CreateEntries[146];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 147:
			createEntry = CreateEntries[147];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 148:
			createEntry = CreateEntries[148];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 149:
			createEntry = CreateEntries[149];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 150:
			createEntry = CreateEntries[150];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 151:
			createEntry = CreateEntries[151];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 152:
			createEntry = CreateEntries[152];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 153:
			createEntry = CreateEntries[153];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 154:
			createEntry = CreateEntries[154];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 155:
			createEntry = CreateEntries[155];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 156:
			createEntry = CreateEntries[156];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 157:
			createEntry = CreateEntries[157];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 158:
			createEntry = CreateEntries[158];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 159:
			createEntry = CreateEntries[159];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 160:
			createEntry = CreateEntries[160];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 161:
			createEntry = CreateEntries[161];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 162:
			createEntry = CreateEntries[162];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 163:
			createEntry = CreateEntries[163];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 164:
			createEntry = CreateEntries[164];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 165:
			createEntry = CreateEntries[165];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 166:
			createEntry = CreateEntries[166];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 167:
			createEntry = CreateEntries[167];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 168:
			createEntry = CreateEntries[168];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 169:
			createEntry = CreateEntries[169];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 170:
			createEntry = CreateEntries[170];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 171:
			createEntry = CreateEntries[171];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 172:
			createEntry = CreateEntries[172];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 173:
			createEntry = CreateEntries[173];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 174:
			createEntry = CreateEntries[174];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 175:
			createEntry = CreateEntries[175];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 176:
			createEntry = CreateEntries[176];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 177:
			createEntry = CreateEntries[177];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 178:
			createEntry = CreateEntries[178];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 179:
			createEntry = CreateEntries[179];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 180:
			createEntry = CreateEntries[180];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 181:
			createEntry = CreateEntries[181];
			HARVEST = [10007, 10008];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 182:
			createEntry = CreateEntries[182];
			HARVEST = [10007, 10008];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 183:
			createEntry = CreateEntries[184];
			HARVEST = [10007, 10008];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 184:
			createEntry = CreateEntries[184];
			HARVEST = [10007, 10008];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 185:
			createEntry = CreateEntries[185];
			HARVEST = [10007, 10008];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		// Start Fletching
		case 190:
			createEntry = CreateEntries[190];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 190:
			createEntry = CreateEntries[190];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 191:
			createEntry = CreateEntries[191];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 192:
			createEntry = CreateEntries[192];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 193:
			createEntry = CreateEntries[193];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 194:
			createEntry = CreateEntries[194];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 195:
			createEntry = CreateEntries[195];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 196:
			createEntry = CreateEntries[196];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 197:
			createEntry = CreateEntries[197];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 198:
			createEntry = CreateEntries[198];
			HARVEST = [10014, 10028];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 199:
			createEntry = CreateEntries[199];
			HARVEST = [10014, 10028];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 200:
			createEntry = CreateEntries[200];
			HARVEST = [10014, 10028];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 201:
			createEntry = CreateEntries[201];
			HARVEST = [10014, 10028];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 202:
			createEntry = CreateEntries[202];
			HARVEST = [10014, 10028];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 203:
			createEntry = CreateEntries[203];
			HARVEST = [10014, 10028];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 204:
			createEntry = CreateEntries[204];
			HARVEST = [10014, 10028];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 205:
			createEntry = CreateEntries[205];
			HARVEST = [10014, 10028];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 206:
			createEntry = CreateEntries[206];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 207:
			createEntry = CreateEntries[207];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 208:
			createEntry = CreateEntries[208];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		//Start Alchemy
		case 290:
			createEntry = CreateEntries[290];
			HARVEST = [10019];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 291:
			createEntry = CreateEntries[291];
			HARVEST = [10019];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 292:
			createEntry = CreateEntries[292];
			HARVEST = [10020];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 293:
			createEntry = CreateEntries[293];
			HARVEST = [10020];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 294:
			createEntry = CreateEntries[294];
			HARVEST = [10020];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 295:
			createEntry = CreateEntries[295];
			HARVEST = [10021];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 296:
			createEntry = CreateEntries[296];
			HARVEST = [10021];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 297:
			createEntry = CreateEntries[297];
			HARVEST = [10021];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 298:
			createEntry = CreateEntries[298];
			HARVEST = [10022];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 299:
			createEntry = CreateEntries[299];
			HARVEST = [10022];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 300:
			createEntry = CreateEntries[300];
			HARVEST = [10022];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 301:
			createEntry = CreateEntries[301];
			HARVEST = [10024];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 302:
			createEntry = CreateEntries[302];
			HARVEST = [10024];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 303:
			createEntry = CreateEntries[303];
			HARVEST = [10024];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 304:
			createEntry = CreateEntries[304];
			HARVEST = [10024];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 305:
			createEntry = CreateEntries[305];
			HARVEST = [10025];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 306:
			createEntry = CreateEntries[306];
			HARVEST = [10025];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 307:
			createEntry = CreateEntries[307];
			HARVEST = [10026];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 308:
			createEntry = CreateEntries[308];
			HARVEST = [10026];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 309:
			createEntry = CreateEntries[309];
			HARVEST = [10023];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
	}

	// Fetch properties of create entry
	var createName = createEntry.name; // name of the create entry
	var createID = createEntry.id; // section id of item to craft
	var addItem = createEntry.addItem; // section header of item to craft
	var sound = createEntry.sound; // sound to play when crafting item
    var delay = createEntry.delay; // how long it takes to craft the item
	var spell = createEntry.spell; // spell requirement to craft the item
	var resources = createEntry.resources; // list of resources needed
	var skills = createEntry.skills; // list of skill requirements
	var avgMinSkill = createEntry.avgMinSkill; // average min skill required
	var avgMaxSkill = createEntry.avgMaxSkill; // average max skill required
	var minRank = createEntry.minRank;
	var maxRank = createEntry.maxRank;

	// List out resource requirements
	for( var i = 0; i < resources.length; i++ )
	{
		var resource = resources[i]
		var amountNeeded = resource[0];
		var resourceColour = resource[1];
        var resourceIDs = resource[2];
	}
	itemdetails( itemGump, pUser );
	itemGump.AddText( 330, 40, textHue, createName );
	itemGump.AddPicture( 20, 50, createID );
	var minSkillReq = 0;
	var maxSkillReq = 0;

	// List out skill requirements
	var primaryCraftSkill = -1;
	var primaryCraftSkillVal = -1;
	var exceptionalChance = -1;
	var rankSum = 0;
	var rank = 0;
	var rndNum1 = 0;
	var supportSkillTooLow = false;
    for( var i = 0; i < skills.length; i++ )
    {
        var skillReq = skills[i];
        var skillNumber = skillReq[0];
		var minSkill = skillReq[1];
		var maxSkill = skillReq[2];

		itemGump.AddHTMLGump( 170, 132 + ( i * 20 ), 200, 18, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 15000 + skills[i][0], socket.language ) + "</basefont> </center>" );
		itemGump.AddText( 430, 132 + ( i * 20 ), textHue, skills[i][1]/10 );

		if( i == 0 )
		{
			primaryCraftSkill = Skills[skillNumber];
			primaryCraftSkillVal = ( pUser.skills[skillNumber] / 10 );
			minSkillReq = minSkill;
			maxSkillReq = maxSkill;
		}
		else
		{
			if( pUser.skills[skillNumber] < minSkill )
			{
				// Support skill doesn't meet minimum requirements!
				supportSkillTooLow = true;
				break;
			}
		}

		if( GetServerSetting( "RankSystem" ))
		{
			// Rough estimate of exceptional chance based on rank system
			var rankRange = maxRank - minRank;
			var skillRange = 60 + pUser.skills[skillNumber] - minSkill;
			if( skillRange < 0 )
			{
				skillRange = minRank * 10;
			}
			else if( skillRange >= 1000 )
			{
				skillRange = maxRank * 10;
			}

			exceptionalChance += skillRange;
		}
	}

	var chance = 0.5 + ((( mainSkill - minSkillReq ) / ( maxSkillReq - minSkillReq )) * 0.5 );
	if( GetServerSetting( "RankSystem" ))
	{
		// Rough estimate for exceptional chance
		exceptionalChance = ( exceptionalChance / skills.length ) / ( 11 - parseInt( GetServerSetting( "SkillLevel" ) / 2 ));
	}

	if( GetServerSetting( "StatsAffectSkillChecks" ))
	{
		// Modify base chance of success with bonuses from stats, if this feature is enabled in ini
		chance += (( pUser.strength * ( primaryCraftSkill.strength / 10 )) / 10000 );
		chance += (( pUser.dexterity * ( primaryCraftSkill.dexterity / 10 )) / 10000 );
		chance += (( pUser.intelligence * ( primaryCraftSkill.intelligence / 10 )) / 10000 );
	}

	for( var i = 0; i < HARVEST.length; i++ )
	{
		itemGump.AddText( 170, 219 + ( i * 20 ), textHue, GetDictionaryEntry( HARVEST[i], socket.language ));
		itemGump.AddText( 430, 219 + ( i * 20 ), textHue, resources[i][0] );
	}

	if( supportSkillTooLow || chance < 0.0 || primaryCraftSkillVal < ( minSkillReq / 10 ))
	{
		chance = 0;
		exceptionalChance = 0;
	}
	else if( chance > 1.0 )
		chance = 1.0;

	itemGump.AddText( 430, 80, textHue, (chance * 100).toFixed( 1 ) + "%" ); // Success Chance:
	if( !exceptionalWearablesOnly || CheckTileFlag( createID, 22 )) // TF_WEARABLE
	{
		if( exceptionalChance == 0 )
		{
			itemGump.AddText( 430, 100, textHue, "0%" ); // Exceptional Chance:
		}
		else
		{
			itemGump.AddText( 430, 100, textHue, Math.min( 100, Math.max(0, (exceptionalChance - 5))).toFixed( 0 ) + "%" + " - " + Math.min( 100, (exceptionalChance + 5)).toFixed( 0 ) + "%" ); // Exceptional Chance:
		}
	}
	else if( exceptionalWearablesOnly || !CheckTileFlag( createID, 22 )) // TF_WEARABLE?
	{
		itemGump.AddText( 430, 100, textHue, "-" ); // No chance of exceptional, not a wearable item!
	}
	itemGump.Send( pUser );
	itemGump.Free();
}

function itemdetails( itemGump, pUser )
{
	var socket = pUser.socket;
	itemGump.AddPage( 0 );
	itemGump.AddBackground( 0, 0, 530, 417, 5054 );
	itemGump.AddTiledGump( 10, 10, 510, 22, 2624 );
	itemGump.AddTiledGump( 10, 37, 150, 148, 2624 );
	itemGump.AddTiledGump( 165, 37, 355, 90, 2624 );
	itemGump.AddTiledGump( 10, 190, 155, 22, 2624 );
	itemGump.AddTiledGump( 10, 240, 150, 57, 2624 );
	itemGump.AddTiledGump( 165, 132, 355, 80, 2624 );
	itemGump.AddTiledGump( 10, 325, 150, 57, 2624 );
	itemGump.AddTiledGump( 165, 217, 355, 80, 2624 );
	itemGump.AddTiledGump( 165, 302, 355, 80, 2624 );
	itemGump.AddTiledGump( 10, 387, 510, 22, 2624 );
	itemGump.AddCheckerTrans( 10, 10, 510, 399 );
    itemGump.AddHTMLGump( 170, 40, 150, 20, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10000, socket.language ) + "</basefont> </center>" );  // ITEM

	itemGump.AddHTMLGump( 10, 217, 150, 22, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10001, socket.language ) + "</basefont> </center>" );  //<CENTER>MATERIALS</CENTER>
	itemGump.AddHTMLGump( 10, 302, 150, 22, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10002, socket.language ) + "</basefont> </center>" );  // <CENTER>OTHER</CENTER>
	itemGump.AddHTMLGump( 170, 80, 250, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10003, socket.language ) + "</basefont>" );  // Success Chance:
	if( GetServerSetting( "RankSystem" ))
	{
		itemGump.AddHTMLGump( 170, 100, 250, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10004, socket.language ) + "</basefont>" );  // Exceptional Chance:
	}
	itemGump.AddButton( 15, 387, 0xfa5, 1, 0, 1 );
	itemGump.AddHTMLGump( 50, 390, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10005, socket.language ) + "</basefont>" );  // BACK
	itemGump.AddHTMLGump( 170, ( 302 + 20 ), 310, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10006, socket.language ) + "</basefont>" );  // * The item retains the color of this material
	itemGump.AddText(  500, 219, textHue, "*"  );
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;

	// Don't continue if character is invalid, or worse... dead!
	if( !ValidateObject( pUser ) || pUser.dead )
		return;

	var bItem = pSock.tempObj;
	var gumpID = scriptID + 0xffff;
	switch( pButton )
	{
		case 0:
			pUser.SetTempTag( "ITEMDETAILS", null )
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;
		case 1:
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
						case 8: // Page 8
						case 9: // Page 9
						case 10: // Page 10
							TriggerEvent( Carpentry, "pageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Carpentry, "pageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 2:
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
							TriggerEvent( Alchemy, "pageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Alchemy, "pageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 3:
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
							TriggerEvent( Fletching, "pageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Fletching, "pageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 4: // Tailoring
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
						case 8: // Page 8
							TriggerEvent( Tailoring, "pageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Tailoring, "pageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 5: // Blacksmithing
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag( "page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
							TriggerEvent( Blacksmithing, "pageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Blacksmithing, "pageX", pSock, pUser, 1 );
							break;
					}
					break;
			}
	}
}