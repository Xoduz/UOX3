function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var itemOwner = GetPackOwner( iUsed, 0 );

	if( socket && iUsed && iUsed.isItem )
	{
		if( itemOwner == null || itemOwner.serial != pUser.serial )
		{
			socket.SysMessage( GetDictionaryEntry( 1763, pSock.language )); // That item must be in your backpack before it can be used.
			return false;
		}
		else if( pUser.skills[45] < 1000 )
		{
			socket.SysMessage( GetDictionaryEntry( 9408, pSock.language )); // Only a Grandmaster Miner can learn from this book.
			return false;
		}
		else if( pUser.GetTag( "GatheringSand" ) == 1 )
		{
			socket.SysMessage( GetDictionaryEntry( 9409, pSock.language )); // You have already learned this information.
			return false;
		}
		else if( iUsed.movable == 2 || iUsed.movable == 3 )
		{
			socket.SysMessage( GetDictionaryEntry( 774, socket.Language )); //That is locked down and you cannot use it
			return false;
		}
		else 
		{
			pUser.SetTag( "GatheringSand", 1 );
			socket.SysMessage( GetDictionaryEntry( 9410, pSock.language )); // You have learned how to mine fine sand.  Target sand areas when mining to look for fine sand.
			iUsed.Delete();
		}
	}
	return false;
}