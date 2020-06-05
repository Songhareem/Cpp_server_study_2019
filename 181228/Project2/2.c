
// 격투게임 공격패턴(스택)
// 커맨드 사이 딜레이시 초기화
// 공격키 눌리는 순간 기준으로 스택을 역으로 읽어서 해당 기술이 있다면 기술 발동
// 아니면 기본 공격
// 공격키는 방향키와 함께 입력되는 경우가 많으나 동시가 아닐 경우 공격키 이후 방향키가 입력되는 경우가 있음
// 이 경우도 기술로 인정해줘야함
// 방금 입력된 키와 새로 입력된 키가 다시 들어온다면(눌려있을 때) 패스(스택에 넣지 않는다)
