function SkillRegistration()
{
	RegisterSkill( 35, true );	// Animal Taming
}

const maxControlSlots = GetServerSetting( "MaxControlSlots" );
const maxFollowers = GetServerSetting( "MaxFollowers" );
const maxPetOwners = GetServerSetting( "MaxPetOwners" );
const maxTimesTamed = 5; // The maximum number of times a pet can be tamed (by different players)
const checkPetControlDifficulty = GetServerSetting( "CheckPetControlDifficulty" );

function onSkill( pUser, objType, skillUsed )
{
	var pSock = pUser.socket;
	if( pSock )
	{
		// Check to see if player actually has space for ANY more pets
		if( maxControlSlots > 0 && pUser.controlSlotsUsed >= maxControlSlots )
		{
			pSock.SysMessage( GetDictionaryEntry( 2397, pSock.language )); // You have too many followers to tame that creature.
			return;
		}

		// See if a creature serial was stored via hard-coded context menu, and use that if so
		var creatureToTame = CalcCharFromSer( parseInt(pUser.GetTag( "tameSerial" )));
		if( ValidateObject( creatureToTame ))
		{
			pUser.SetTag( "tameSerial", null );
			onCallback0( pSock, creatureToTame );
		}
		else
		{
			pSock.CustomTarget( 0, GetDictionaryEntry( 859, pSock.language ) );
		}
	}

	return true;
}

function onCallback0( pSock, ourObj )
{
	var pUser = pSock.currentChar;
	if( ValidateObject( ourObj ) && ourObj.isChar && ValidateObject( pUser ))
	{
		var skillToTame = ourObj.skillToTame;
		var pLanguage 	= pSock.language;

		var hasBeenOwner = ourObj.HasBeenOwner( pUser );

		if( !skillToTame )
		{
			pSock.SysMessage( GetDictionaryEntry( 1593, pLanguage ) ); // You can't tame that creature
			return;
		}
		else if( !hasBeenOwner && skillToTame > pUser.skills.taming )
		{
			pSock.SysMessage( GetDictionaryEntry( 2398, pLanguage ) ); // You are not skilled enough to tame that creature
			return;
		}

		var controlSlots = ourObj.controlSlots;
		if( maxControlSlots > 0 && ( pUser.controlSlotsUsed + controlSlots > maxControlSlots ))
		{
			pSock.SysMessage( GetDictionaryEntry( 2390, pSock.language )); // That would exceed your maximum number of pet control slots.
			return;
		}

		if( pUser.petCount >= maxFollowers )
		{
			var tempMsg = GetDictionaryEntry( 2346, pSock.language ); // You can maximum have %i pets/followers active at the same time.
			pSock.SysMessage( tempMsg.replace(/%i/gi, maxFollowers ));
			return;
		}

		if( !hasBeenOwner && maxPetOwners > 0 && ( ourObj.ownerCount >= maxPetOwners ))
		{
			pSock.SysMessage( GetDictionaryEntry( 2402, pSock.language )); // The creature has had too many masters and is not willing to do the bidding of another one!
			return;
		}

		if( !ourObj.InRange( pUser, 3 ) )
			pSock.SysMessage( GetDictionaryEntry( 393, pLanguage ) ); // That is out of range
		else if( ourObj.tamed )
		{
			if( ourObj.owner && ourObj.owner.serial == pUser.serial )
				pSock.SysMessage( GetDictionaryEntry( 1594, pLanguage ) ); // You already control that creature!
			else
				pSock.SysMessage( GetDictionaryEntry( 1595, pLanguage ) ); // That creature is already controlled by another!
		}
		else if( !pUser.CanSee( ourObj ))
		{
			pSock.SysMessage( GetDictionaryEntry( 2395, pSock.language )); // You do not have a clear path to the animal you are taming, and must cease your attempt.
		}
		else
		{
			pSock.tempObj2 = ourObj;
			pUser.TextMessage( GetDictionaryEntry( 2392, pSock.language ), false, 0x59 ); // You start to tame the creature
			var tempMsg = GetDictionaryEntry( 1596, pSock.language ) // *%s starts to tame %t*
			tempMsg = tempMsg.replace(/%s/gi, pUser.name )
			pUser.EmoteMessage( tempMsg.replace(/%t/gi, ourObj.name ) );

			pUser.StartTimer( 3000, 1, true );
		}
	}
	else
		pSock.SysMessage( GetDictionaryEntry( 1603, pSock.language )); // You can't tame that!
}

function onTimer( pUser, timerID )
{
	if( !RunTameChecks( pUser ))
		return;

	var toTame = pUser.socket.tempObj2;
	if( !ValidateObject( toTame ))
		return;

	if( timerID == 1 )
	{
		SayTameMessage( pUser, toTame );
		if( !CheckTameSuccess( pUser, toTame ))
			pUser.StartTimer( 3000, 2, true );
	}
	else if( timerID == 2 )
	{
		SayTameMessage( pUser, toTame );
		if( !CheckTameSuccess( pUser, toTame ))
			pUser.StartTimer( 3000, 3, true );
	}
	else if( timerID == 3 )
	{
		SayTameMessage( pUser, toTame );
		if( !CheckTameSuccess( pUser, toTame ))
		{
			pUser.socket.SysMessage( GetDictionaryEntry( 1601, pUser.socket.language ) ); //You were unable to tame it.
			pUser.socket.tempObj2 = null;
		}
	}
}

function CheckTameSuccess( pUser, toTame )
{
	var hasBeenOwner = toTame.HasBeenOwner( pUser );

	if( !hasBeenOwner && !pUser.CheckSkill( 35, toTame.orneriness, 1000 ))
	{
		return false;
	}
	else
	{
		pUser.socket.tempObj2 = null;
		toTame.TextMessage( GetDictionaryEntry( 1602, pUser.socket.language ), false, 0x3b2, 0, pUser.serial ); // It seems to accept you as its master!
		toTame.owner 		= pUser;
		toTame.wandertype 	= 0;
		toTame.aitype		= 0;
		toTame.tamed		= true;
		toTame.loyalty 		= 25; // start at 25 out of 100

		var hasBeenOwner = toTame.HasBeenOwner( pUser );
		if( checkPetControlDifficulty && hasBeenOwner )
		{
			// Chance to calm down pet and reduce "orneriness" based on animal lore skill
			if( pUser.CheckSkill( 2, toTame.orneriness, 1000 ))
			{
				switch( toTame.ownerCount )
				{
					case 1:
						toTame.orneriness = toTame.skillToTame;
						break;
					case 2:
						toTame.orneriness = toTame.skillToTame + 48;
						break;
					case 3:
						toTame.orneriness = toTame.skillToTame + 192;
						break;
					case 4:
						toTame.orneriness = toTame.skillToTame + 432;
						break;
					case 5:
						toTame.orneriness = toTame.skillToTame + 768;
						break;
					default:
						break;
				}
			}
		}

		toTame.Follow( pUser );
		pUser.controlSlotsUsed = pUser.controlSlotsUsed + toTame.controlSlots;
		if( toTame.atWar )
		{
			toTame.target 	= null;
			toTame.atWar	= false;
		}
		if( pUser.atWar && ValidateObject( pUser.target ) && pUser.target.serial == toTame.serial )
		{
			pUser.target	= null;
			pUser.atWar	= false;
		}
		return true;
	}
}

function SayTameMessage( pUser, toTame )
{
	var pSock = pUser.socket;
	if( pSock == null )
		return;

	switch( RandomNumber( 0, 3 ) )
	{
		case 0: pUser.TextMessage( GetDictionaryEntry( 1597, pSock.language ) );	break; // I've always wanted a pet like you.
		case 1: pUser.TextMessage( GetDictionaryEntry( 1598, pSock.language ) );	break; // Will you be my friend?
		case 2:
		{
			var tempMsg = GetDictionaryEntry( 1599, pSock.language ) // Here %s.
			tempMsg = tempMsg.replace(/%s/gi, toTame.name );
			tempMsg = tempMsg.replace('a ', '');
			tempMsg = tempMsg.replace('an ', '');
			pUser.TextMessage( tempMsg );
			break;
		}
		case 3:
		{
			var tempMsg = GetDictionaryEntry( 1600, pSock.language ) // Good %s.
			tempMsg = tempMsg.replace(/%s/gi, toTame.name );
			tempMsg = tempMsg.replace('a ', '');
			tempMsg = tempMsg.replace('an ', '');
			pUser.TextMessage( tempMsg );
			break;
		}
	}
}

function RunTameChecks( pUser )
{
	var pSock = pUser.socket;
	if( pSock == null )
		return false;

	var toTame = pSock.tempObj2;
	if( !ValidateObject( toTame ))
		return false;

	// Unhide player if they're hidden
	if( pUser.visible > 0 && pUser.visible < 3 )
	{
		pUser.visible = 0;
		pUser.stealth = -1;
		pUser.SetTimer( Timer.INVIS, 0 );
	}

	var controlSlots = toTame.controlSlots;
	if( maxControlSlots > 0 && ( pUser.controlSlotsUsed + controlSlots > maxControlSlots ))
	{
		pSock.SysMessage( GetDictionaryEntry( 2390, pSock.language )); // That would exceed your maximum number of pet control slots.
		return false;
	}

	if( pUser.petCount >= maxFollowers )
	{
		var tempMsg = GetDictionaryEntry( 2346, pSock.language ); // You can maximum have %i pets/followers active at the same time.
		pSock.SysMessage( tempMsg.replace(/%i/gi, maxFollowers ));
		return false;
	}

	if( !pUser.InRange( toTame, 8 ))
	{
		pUser.TextMessage( GetDictionaryEntry( 2393, pSock.language ), false, 0x3b2 ); // You are too far away to continue taming.
		return false;
	}
	else if( pUser.dead )
	{
		pUser.TextMessage( GetDictionaryEntry( 2394, pSock.language ), false, 0x3b2 ); // You are dead, and cannot continue taming.
		return false;
	}
	else if( !pUser.CanSee( toTame ))
	{
		pSock.SysMessage( GetDictionaryEntry( 2395, pSock.language )); // You do not have a clear path to the animal you are taming, and must cease your attempt.
		return false;
	}
	else if( !toTame.skillToTame )
	{
		pUser.TextMessage( GetDictionaryEntry( 1593, pSock.language ), false, 0x3b2 ); // You can't tame that creature.
		return false;
	}
	else if( toTame.tamed )
	{
		toTame.TextMessage( GetDictionaryEntry( 1595, pSock.language ), false, 0x3b2, 0, pUser.serial ); // That creature is already controlled by another!
		return false;
	}
	else if( toTame.ownerCount > maxPetOwners ) // Creature had too many owners already
	{
		var hasBeenOwner = toTame.HasBeenOwner( pUser );
		if( !hasBeenOwner )
		{
			toTame.TextMessage( GetDictionaryEntry( 2396, pSock.language ), false, 0x3b2, 0, pUser.serial ); // This animal has had too many owners and is too upset for you to tame.
			return false;
		}
	}
	/*else if( toTame.needsSubduing ) // Creature needs to be subdued before it can be tamed
	{

	}*/
	return true;
}
